import { useSearchParams } from '../../../../../common/hooks'
import { OptionBox } from '../../../../../common/components'

export function createTabContent({ Selects, List, getDefaultParams }) {
  return function TabContent() {
    const [params, actions] = useSearchParams(getDefaultParams())

    return (
      <div className='fill flex-y'>
        <OptionBox paramsActions={actions} params={params}>
          <Selects />
        </OptionBox>
        <List params={params} />
      </div>
    )
  }
}
