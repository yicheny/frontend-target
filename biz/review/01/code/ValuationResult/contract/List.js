import { DataGrid, Card } from 'rootnet-ui'
import { DownCsv } from '../../../../common/components'
import { useGetColumns, useInitFetchContract } from '../widgets/hooks'
import _ from 'lodash'

function ContractCard({ search, params, pageMode, selectOption }) {
  const { swappaymentType } = selectOption || {}
  const columns = useGetColumns('contract', swappaymentType)
  const { data, loading, error } = useInitFetchContract(params, pageMode)
  const extra = <DownCsv tit='合约价值' value={data} options={_.get(columns, 'columns', [])} label text='下载' />

  return (
    <Card title='合约价值' className='flex-y x-card-singlegrid' extra={extra} loading={loading} error={error}>
      <DataGrid option={columns} data={data} className='row-fill hl_header' />
    </Card>
  )
}

export default ContractCard

// const { data, pageSize, total, pageNum, loading, error } = useInitFetchContract(params, pageMode)
// {/* <Pagination
//         pageSize={pageSize}
//         total={total}
//         current={pageNum}
//         selector={[20, 50, 100, 500]}
//         onChange={(pageNum, pageSize) => search((x) => ({ ...x, pageSize, pageNum }))}
//       />*/}
